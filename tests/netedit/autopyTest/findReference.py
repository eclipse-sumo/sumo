import autopy
import time


def find_image_example():
    reference = autopy.bitmap.Bitmap.open('reference.png')
    
    print(reference)
    width, height = autopy.screen.size()
    print(width*autopy.screen.scale())
    print(height*autopy.screen.scale())
    print(autopy.screen.scale())

    thistuple2 = ((0,0), (width, height))

    for x in range(0, 10):
        pos = autopy.bitmap.capture_screen().find_bitmap(reference, 0, thistuple2)
        if pos:
            print("Found needle at: %s" % str(pos))
        time.sleep(1)

find_image_example()