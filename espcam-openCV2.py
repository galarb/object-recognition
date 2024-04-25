import cv2
import numpy as np
import urllib.request
import requests  # Import requests library for making HTTP requests
from time import sleep
url = 'http://192.168.86.26/cam-hi.jpg'

cap = cv2.VideoCapture(url)
whT=320
confThreshold = 0.4
nmsThreshold = 0.3
classesfile='coco.names'
classNames=[]

with open(classesfile,'rt') as f:
    classNames=f.read().rstrip('\n').split('\n')
#print(classNames)

modelConfig = 'yolov3.cfg'
modelWeights= 'yolov3.weights'
net = cv2.dnn.readNetFromDarknet(modelConfig,modelWeights)
net.setPreferableBackend(cv2.dnn.DNN_BACKEND_OPENCV)
net.setPreferableTarget(cv2.dnn.DNN_TARGET_CPU)
output_layers = net.getUnconnectedOutLayersNames()
network = cv2.dnn.readNetFromDarknet('yolov3.cfg', 'yolov3.weights')
layers = network.getLayerNames()
yolo_layers = [layers[i - 1] for i in network.getUnconnectedOutLayers()]

# Initialize variables to track detected objects and their locations
found_laptop = False
found_mouse = False
found_cell_phone = False
laptop_bbox = []
mouse_bbox = []
cell_phone_bbox = []

def send_alert_to_esp(label, x, y, frame_width, frame_height):
    # Calculate the center of the frame
    center_x = frame_width // 2
    center_y = frame_height // 2
    
    # Calculate the adjusted x and y coordinates
    adjusted_x = x - (frame_width // 2)
    adjusted_y = y - (frame_height // 2)
    
    print("Sending alert to ESP with label:", label, "and adjusted coordinates (x, y):", adjusted_x, adjusted_y)
    # URL of the ESP32 endpoint to receive the alert
    esp_endpoint = 'http://192.168.86.26/alert'  # Update with your ESP32 IP address and endpoint
    
    # Data to send in the request
    data = {'label': label, 'x': adjusted_x, 'y': adjusted_y}  # Include label, adjusted x, and adjusted y in the data
    
    # Send POST request to the ESP32 endpoint
    response = requests.post(esp_endpoint, data=data)
    
    # Check if the request was successful
    if response.status_code == 200:
        print('Alert sent to ESP32 successfully')
    else:
        print('Failed to send alert to ESP32')

def findObject(outputs, im):
    global found_laptop, found_mouse, found_cell_phone  # Declare variables as global
    hT, wT, cT = im.shape
    bbox = []
    classIds = []
    confs = []
    found_laptop = False
    found_mouse = False
    found_cell_phone = False
    for output in outputs:
        for det in output:
            scores = det[5:]
            classId = np.argmax(scores)
            confidence = scores[classId]
            if confidence > confThreshold:
                w, h = int(det[2] * wT), int(det[3] * hT)
                x, y = int((det[0] * wT) - w / 2), int((det[1] * hT) - h / 2)
                
                bbox.append([x, y, w, h])
                classIds.append(classId)
                confs.append(float(confidence))
    
    indices = cv2.dnn.NMSBoxes(bbox, confs, confThreshold, nmsThreshold)
    if len(indices) > 0:
        indices = indices.flatten()
        for i in indices:
            box = bbox[i]
            x, y, w, h = box[0], box[1], box[2], box[3]
            label = classNames[classIds[i]]
            
            print("Detected object:", label)  # Print label
            
            # Check for specific object classes
            if label == 'laptop':
                found_laptop = True
            elif label == 'mouse':
                found_mouse = True
            elif label == 'cell phone':
                found_cell_phone = True
            
            if found_cell_phone or found_mouse or found_laptop:
                # Calculate the center of the detected object's bounding box
                center_x = x + w // 2
                center_y = y + h // 2
                
                # Calculate adjusted x and y coordinates based on the center of the frame
                center_frame_x = wT // 2
                center_frame_y = hT // 2
                adjusted_x = center_x - center_frame_x
                adjusted_y = center_y - center_frame_y
                send_alert_to_esp(label, adjusted_x, adjusted_y, wT, hT)  # Send alert with adjusted x and y

            # Draw bounding box and label
            cv2.rectangle(im, (x, y), (x + w, y + h), (255, 0, 255), 2)
            cv2.putText(im, f'{label.upper()} {int(confs[i]*100)}%', 
                        (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 0, 255), 2)
            
    if found_laptop or found_mouse or found_cell_phone:
        print('Alert: objects detected')






while True:
    img_resp = urllib.request.urlopen(url)
    imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
    im = cv2.imdecode(imgnp, -1)
    success, img = cap.read()
    blob = cv2.dnn.blobFromImage(im, 1/255, (whT, whT), [0, 0, 0], 1, crop=False)
    net.setInput(blob)
    layernames = net.getLayerNames()
    outputNames = [layernames[layerId - 1] for layerId in net.getUnconnectedOutLayers()]
    outputs = net.forward(outputNames)
    findObject(outputs, im)
    sleep(1)
        
    cv2.imshow('IMage', im)
    cv2.waitKey(1)
cap.release()
cv2.destroyAllWindows()