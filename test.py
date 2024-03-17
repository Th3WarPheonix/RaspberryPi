
# Import OpenCV library 
import cv2 
import numpy as np
import time
  
# Read an Image 
img = cv2.imread("/home/TheWarPheonix/Pictures/Cascade.JPG", 
                 cv2.IMREAD_COLOR) 
  
# Display image using imshow() function
# cv2.imshow("I2999", np.zeros([10,10], dtype=np.uint8))
# cv2.moveWindow("I2999", 3000, 50)
# cv2.destroyAllWindows()

poss = np.linspace(0, 2000, 11, dtype=int)

for i, pos in enumerate(poss):
    print(pos)
    # Display image using imshow() function 
    cv2.imshow(f'{pos}', np.zeros([10,10], dtype=np.uint8)) 
    # Move window to (10,50) position 
    # using moveWindow() function 
    cv2.moveWindow(f'{pos}', pos, 50)

    # Wait for user to press any key 
    cv2.waitKey(0) 
    
    # Close all opened windows 
    cv2.destroyAllWindows()
    