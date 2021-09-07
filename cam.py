import cv2 as cv
from matplotlib import pyplot
import numpy as np
import pytesseract
import serial
import time

def nothing(a):
    pass


if __name__ == '__main__':
    lang = 'lets'
    custom_config = r'--oem 3 --psm 6 -c tessedit_char_whitelist=0123456789'

    window = 'original'
    crop_bar = 'crop'
    out = 'out'

    cv.namedWindow(window)
    cv.namedWindow(crop_bar)
    cv.namedWindow(out)

    vid = cv.VideoCapture(0)
    vid.set(cv.CAP_PROP_FRAME_WIDTH, 1920)
    vid.set(cv.CAP_PROP_FRAME_HEIGHT, 1080)
    cv.createTrackbar('Gauss', window, 2, 30, nothing)
    cv.createTrackbar('kernel', window, 2, 30, nothing)
    cv.createTrackbar('focus', window, 5, 50, nothing)
    cv.createTrackbar('x0', window, 587, 1080, nothing)
    cv.createTrackbar('x1', window, 685, 1080, nothing)
    cv.createTrackbar('y0', window, 774, 1920, nothing)
    cv.createTrackbar('y1', window, 963, 1920, nothing)

    while True:
        t0 = time.time()
        gauss = cv.getTrackbarPos('Gauss', window)
        kernel = cv.getTrackbarPos('kernel', window)
        focus = cv.getTrackbarPos('focus', window) * 5
        x0 = cv.getTrackbarPos('x0', window)
        x1 = cv.getTrackbarPos('x1', window)
        y0 = cv.getTrackbarPos('y0', window)
        y1 = cv.getTrackbarPos('y1', window)
        vid.set(28, focus)
        ret, image = vid.read()
        cv.imshow(window, image)

        image = image[x0:x1, y0:y1]
        gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)

        if gauss > 0:
            gray = cv.blur(gray, (gauss, gauss))

        print('mean grey - ', np.mean(gray))
        thresh = cv.threshold(gray, 0, 255,
                              cv.THRESH_BINARY | cv.THRESH_OTSU)[1]
        #thresh = cv.threshold(gray, 127, 255, cv.THRESH_BINARY)[1]
        #thresh = cv.adaptiveThreshold(gray, 255, cv.ADAPTIVE_THRESH_GAUSSIAN_C, \
        #                           cv.THRESH_BINARY, 11, 2)
        if kernel > 0:
            kernel = cv.getStructuringElement(cv.MORPH_ELLIPSE, (kernel, kernel))
            # thresh = cv.morphologyEx(thresh, cv.MORPH_OPEN, kernel)
            thresh = cv.erode(thresh, kernel, iterations=1)
        print('mean thresh - ', np.mean(thresh))
        details = pytesseract.image_to_string(thresh,
                                              config=custom_config,
                                              lang=lang)
        cv.putText(image, details[0:3],
                   (50, 50),
                   cv.FONT_HERSHEY_SIMPLEX,
                   1,
                   (0, 255, 0),
                   2)
        cv.imshow(out, image)
        cv.imshow(crop_bar, thresh)
        t = time.time()
        print(1/(t - t0))
        print(t)
        if cv.waitKey(1) == ord('q'):
            break
    vid.release()
    cv.destroyAllWindows()
