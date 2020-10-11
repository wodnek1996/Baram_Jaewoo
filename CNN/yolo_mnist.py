import torch.nn.init
import cv2
import numpy as np
import torchvision.transforms as T
import time

import torch
import torchvision
from torchvision import transforms

if torch.cuda.is_available :
    device = 'cuda'
else:
    device = 'cpu'

class CNN(torch.nn.Module):

    def __init__(self):
        super(CNN, self).__init__()
        self.layer1 = torch.nn.Sequential(
            torch.nn.Conv2d(1, 32, kernel_size=3, stride=1, padding=1),
            torch.nn.ReLU(),
            torch.nn.MaxPool2d(kernel_size=2, stride=2))

        self.layer2 = torch.nn.Sequential(
            torch.nn.Conv2d(32, 64, kernel_size=3, stride=1, padding=1),
            torch.nn.ReLU(),
            torch.nn.MaxPool2d(kernel_size=2, stride=2))

        self.fc = torch.nn.Linear(7 * 7 * 64, 10, bias=True)
        torch.nn.init.xavier_uniform_(self.fc.weight)

    def forward(self, x):
        out = self.layer1(x)
        out = self.layer2(out)
        out = out.view(out.size(0), -1)   # Flatten them for FC
        out = self.fc(out)
        return out

def cnn_pytorch(img_input, x, y, h, w):
    img_roi = img_input[(y-10):(y + h +20), (x-20):(x + w+15)]
    cv2.imshow('cnn', img_roi)
    time.sleep(0.5)
    image = cv2.cvtColor(img_roi, cv2.COLOR_BGR2GRAY)
    data = cv2.resize(image, dsize=(28, 28), interpolation=cv2.INTER_AREA)
    img_data = data / 256.0
    Data = torch.as_tensor(img_data)
    Data_1 = Data.reshape((1, 1, 28, 28)).float().to(device)
    prediction = model(Data_1)
    pre = torch.argmax(prediction, 1)
    print('prediction : ', pre)
    #cv2.putText(img_roi, pre, (10,10), cv2.FONT_HERSHEY_PLAIN, 1, (0,255,0), 3)
    cv2.imshow('cnn', img_roi)

def yolo(img_color, output_layers, colors ):
    # Detecting objects
    blob = cv2.dnn.blobFromImage(img_color, 0.00392, (416, 416), (0, 0, 0), True, crop=False)
    net.setInput(blob)
    outs = net.forward(output_layers)

    # 정보를 화면에 표시
    class_ids = []
    confidences = []
    boxes = []
    for out in outs:
        for detection in out:
            scores = detection[5:]
            class_id = np.argmax(scores)
            confidence = scores[class_id]
            if confidence > 0.5:
                # Object detected
                center_x = int(detection[0] * width)
                center_y = int(detection[1] * height)
                w = int(detection[2] * width)
                h = int(detection[3] * height)
                # 좌표
                x = int(center_x - w / 2)
                y = int(center_y - h / 2)
                boxes.append([x, y, w, h])
                confidences.append(float(confidence))
                class_ids.append(class_id)

    indexes = cv2.dnn.NMSBoxes(boxes, confidences, 0.3, 0.4)

    font = cv2.FONT_HERSHEY_PLAIN
    for i in range(len(boxes)):
        if i in indexes:
            x, y, w, h = boxes[i]
            label = str(classes[class_ids[i]])
            color = colors[i]
            cv2.rectangle(img_color, (x, y), (x + w, y + h),color, 2)
            cv2.putText(img_color, label, (x, y + 30), font, 3, color, 3)
            if (len(class_ids) > 3):
                if class_ids[i] == 1:
                    cnn_pytorch(img_input, x, y, h, w)
                    #time.sleep(0.5)



device = torch.device("cuda")
model = CNN().cuda()
model.load_state_dict(torch.load("./test_model.pth"))
model.eval()

# Yolo 로드
net = cv2.dnn.readNet("yolo-obj_last.weights", "yolo-obj.cfg")
classes = []
with open("obj.names", "r") as f:
    classes = [line.strip() for line in f.readlines()]
layer_names = net.getLayerNames()
output_layers = [layer_names[i[0] - 1] for i in net.getUnconnectedOutLayers()]
colors = np.random.uniform(0, 255, size=(50, 3))

cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)

while (True):

    ret, img_color = cap.read()

    if ret == False:
        break;

    img_input = img_color.copy()
    cv2.imshow("Image", img_color)
    height, width, channels = img_color.shape

    yolo(img_color, output_layers, colors)

    cv2.imshow("Image", img_color)
    print('하나 끝!')

    key = cv2.waitKey(1)
    if key == 27:
        break

cv2.waitKey(0)
cap.release()
cv2.destroyAllWindows()
