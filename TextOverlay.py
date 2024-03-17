import cv2
from picamera2 import Picamera2
import time

"""
Source
TopTechBoy.com
Paul McWhorter
https://www.youtube.com/watch?v=_3MXdbry2yo&list=PLGs0VKk2DiYxdMjCJmcP6jt4Yw6OHK85O&index=52
"""

def main()
    picam2 = Picamera2()
    dispW = 1280
    dispH = 720
    framerate = 30 # caps at 50
    trustold = .9 # low pass filter trust value
    
    picam2.preview_configuration.main.size = (dispW,dispH)
    picam2.preview_configuration.main.format = "RGB888" # the way opencv reads in the pixel data
    picam2.preview_configuration.controls.FrameRate = framerate
    picam2.preview_configuration.align() # get closest canonical resolution
    picam2.configure("preview")
    picam2.start()

    fps = 0
    pos1 = (30,60) # position of top left corner of first text box
    pos2 = (0,15) # position of top left corner of second text box
    font = cv2.FONT_HERSHEY_PLAIN
    height = 1.5
    weight = int(2*height)

    myColor1 = (255,255,255) # BGR
    myColor2 = (125,125,125) # BGR

    print((picam2.capture_array()).size)
    while True:
        tStart=time.time()
        image = picam2.capture_array() # returns array of rgb values per pixel, the total array size is dispW*dispH*3
        
        cv2.putText(image,str(int(fps))+' FPS',pos1,font,height,myColor1,weight)
        
        cv2.putText(image,str(int(fps))+' FPS',pos2,font,height,myColor2,weight)
        
        cv2.imshow("Camera", image)
        if cv2.waitKey(1)==ord('q'):
            break
        tEnd=time.time()
        loopTime=tEnd-tStart
        
        fps=trustold*fps + (1-trustold)*(1/loopTime) # how much you trust the old (90%) vs the new fps (10%) similar to a low pass filter
        
    cv2.destroyAllWindows()
    
if __name__ == '__main__':
    main()
