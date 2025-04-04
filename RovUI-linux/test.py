import cv2 as cv
from time import sleep

cap = cv.VideoCapture('rtsp://192.168.1.6/stream=0') 

print("Hello from OpenCV", cv.__version__)

while True:
   ok, frame = cap.read()

   if not ok:
       print("Can't receive frame")
       cv.waitKey(1000)
   else:
       cv.imshow("", frame)
       cv.waitKey(1)