import cv2
from picamera2 import Picamera2
import time
import numpy as np

"""
Source
TopTechBoy.com
Paul McWhorter
https://www.youtube.com/watch?v=_3MXdbry2yo&list=PLGs0VKk2DiYxdMjCJmcP6jt4Yw6OHK85O&index=52
"""

def main():
    cv2.imshow("I2999", np.zeros([10,10], dtype=np.uint8))
    cv2.moveWindow("I2999", 3000, 50)
    cv2.destroyAllWindows()
    
    picam2 = Picamera2()

    # Image settings
    dispW = 1280
    dispH = 720
    pos2 = (-1,719) # position of top left corner of second text box

    # Frame rate calculationn settings
    framerate = 30 # caps at 50
    trustold = .9 # low pass filter trust value
    fps = 0

    # Text settings
    font = cv2.FONT_HERSHEY_PLAIN
    height = 1.5
    weight = int(.75*height)
    green = (0, 225, 0)
    
    # Cmaera set up
    picam2.preview_configuration.main.size = (dispW,dispH)
    picam2.preview_configuration.main.format = "RGB888" # the way opencv reads in the pixel data
    picam2.preview_configuration.controls.FrameRate = framerate
    picam2.preview_configuration.align() # get closest canonical resolution
    picam2.configure("preview")
    picam2.start()

    while True:
        tStart = time.time()
        image = picam2.capture_array() # returns array of rgb values per pixel, the total array size is dispW*dispH*3
                
        cv2.putText(image,str(int(fps))+' FPS',pos2,font,height,green,weight)
        
        cv2.imshow("Camera", image)
        cv2.moveWindow("Camera", 50, 0)
        if cv2.waitKey(1) == ord('q'):
            break
        tEnd = time.time()
        loopTime = tEnd-tStart
        
        fps = trustold*fps + (1-trustold)*(1/loopTime) # how much you trust the old (90%) vs the new fps (10%) similar to a low pass filter
        
    cv2.destroyAllWindows()
    
if __name__ == '__main__':
    main()
