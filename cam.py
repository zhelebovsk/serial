import cv2 as cv
from matplotlib import pyplot
import numpy as np
import pytesseract
import serial


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
    vid.set(cv.CAP_PROP_FRAME_WIDTH, 1280)
    vid.set(cv.CAP_PROP_FRAME_HEIGHT, 720)
    cv.createTrackbar('Gauss', window, 5, 30, nothing)
    cv.createTrackbar('kernel', window, 5, 30, nothing)

    cv.createTrackbar('x0', crop_bar, 0, 1000, nothing)
    cv.createTrackbar('x1', crop_bar, 240, 1000, nothing)
    cv.createTrackbar('y0', crop_bar, 590, 1000, nothing)
    cv.createTrackbar('y1', crop_bar, 700, 1000, nothing)


    while True:
        gauss = cv.getTrackbarPos('Gauss', window)
        kernel = cv.getTrackbarPos('kernel', window)

        x0 = cv.getTrackbarPos('x0', crop_bar)
        x1 = cv.getTrackbarPos('x1', crop_bar)
        y0 = cv.getTrackbarPos('y0', crop_bar)
        y1 = cv.getTrackbarPos('y1', crop_bar)

        ret, image = vid.read()
        cv.imshow(window, image)

        image = image[x0:x1, y0:y1]
        gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)

        if gauss > 0:
            gray = cv.blur(gray, (gauss, gauss))
        thresh = cv.threshold(gray, 0, 255,
                              cv.THRESH_BINARY | cv.THRESH_OTSU)[1]
        if kernel > 0:
            kernel = cv.getStructuringElement(cv.MORPH_ELLIPSE, (kernel, kernel))
            # thresh = cv.morphologyEx(thresh, cv.MORPH_OPEN, kernel)
            thresh = cv.erode(thresh, kernel, iterations=1)

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

        if cv.waitKey(1) == ord('q'):
            break
    vid.release()
    cv.destroyAllWindows()
