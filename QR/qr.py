#!/usr/bin/env python
import pyzbar.pyzbar as pyzbar
import cv2
import rospy from std_msgs.msg import String
import sys

def qr_cord():
     cap = cv2.VideoCapture(0)

     while (cap.isOpened()):
             ret, img = cap.read()
             pub = rospy.Publisher('chatter',String,queue_size=10)
             rospy.init_node('talker', anonymous = True)

             if not ret:
                          continue

             gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
             decoded = pyzbar.decode(gray)

             for d in decoded:
                          x, y, w, h = d.rect
                          barcode_data = d.data.decode("utf-8")
                          barcode_type = d.type
                          cv2.rectangle(img, (x, y), (x + w, y + h), (0, 0, 255), 2)
                          text = '%s (%s)' % (barcode_data, barcode_type)
                          cv2.putText(img, text, (x, y), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 255), 2, cv2.LINE_AA)
                          send_str = barcode_data
                          rospy.loginfo(send_str)
                          pub.publish(send_str)
                          rospy.sleep(0.25)
             cv2.imshow('img', img)

             key = cv2.waitKey(1)
             if key == ord('q'):
                          break

if __name__=='__main__':
             while(1):
                          try:qr_cord()
                          except rospy.ROSInterruptException:pass
cap.release()
cv2.destroyAllWindows()
