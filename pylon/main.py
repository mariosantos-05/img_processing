import cv2
import numpy as np

#Input e Parametros

image = cv2.imread("moire.tif", cv2.IMREAD_GRAYSCALE)  

width = image.shape[0]
height = image.shape[1]

fourier_transformed = cv2.dft(np.float32(image))
fourier_transformed_shifted = np.fft.fftshift(fourier_transformed)


output = fourier_transformed_shifted

cv2.namedWindow("Imagem", cv2.WINDOW_NORMAL)
cv2.resizeWindow("Imagem", height, width)
cv2.imshow("Imagem", output)
cv2.waitKey(0)
cv2.destroyAllWindows()
