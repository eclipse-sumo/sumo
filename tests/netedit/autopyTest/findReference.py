import pyautogui
import time


def find_reference_example():

    # aprox 30-40 second for search  reference (every locateOnScreen takes about 1-2 seconds)
    for x in range(0, 15):
        # search reference
        position = pyautogui.locateOnScreen('reference.png')
        # check if pos was found
        if position:
            # break loop
            print("Found reference at position: %s" % str(position))
            break
        # wait two second
        time.sleep(1)


find_reference_example()
