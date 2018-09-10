import autopy
import time


def find_reference_example():
    # get reference
    reference = autopy.bitmap.Bitmap.open('reference.png')

    # write width and height
    width, height = autopy.screen.size()
    print(width*autopy.screen.scale())
    print(height*autopy.screen.scale())

    # write screen scale
    print(autopy.screen.scale())

    # 30 second for search  reference
    for x in range(0, 15):
        # capture screen and search reference
        pos = autopy.bitmap.capture_screen().find_bitmap(reference)
        # check if pos was found
        if pos:
            # break loop
            print("Found reference at position: %s" % str(pos))
            break
        # wait two second
        time.sleep(2)

find_reference_example()