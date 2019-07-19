import numpy as np
import matplotlib.pyplot as plt
from scipy import misc
from scipy.ndimage import gaussian_filter
import sys
from skimage import color
from skimage import filters

plt.rcParams['image.cmap'] = 'gray'


img = color.rgb2gray(misc.imread(sys.argv[1]))
sigma = (min(img.shape[0],img.shape[1])/10)*float(sys.argv[2])
print("using sigma = "+str(sigma))
#plt.imshow(img)
#plt.show()
print("extracting gradient...")
g = gaussian_filter(img,sigma)
print("...done")
#plt.imshow(g)
#plt.show()
print("inverting gradient...")
g2 = 255-g
print("...done")
#plt.imshow(g2)
#plt.show()
print("removing gradient...")
r = (img+g2)/2
print("...done")
#plt.imshow(r)
#plt.show()
print("denoising result...")
r = gaussian_filter(r,2)
print("...done")
print("finding threshold...")
val = filters.threshold_otsu(r)
print("...done\nthreshold is "+str(val))
plt.imshow(r>val)
plt.show()
out = r>val

misc.imsave(sys.argv[1]+"_binarized.png",out.astype(int))
print("wrote binarized image to "+ sys.argv[1].split(".")[0]+"_binarized.png")
