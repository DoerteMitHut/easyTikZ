# easyTikz-README
## Collaborating on the Progress document
### Overleaf
* [The edit link for overleaf](https://www.overleaf.com/9582212296gqwnwjcchzjc)

### Using the git submodule
Get the most recent version of the submodule by cloning via `git clone --recurse-submodules https://hal.emsec.rub.de/hoffmann/easytikz.git`.  
Alternatively, after cloning the project use `git submodule init` and `git submodule update` from the main directory.  
In order to pull changes to the submodule use `git submodule update --remote`  

Pulling from or pushing to the overleaf repo should ask for your overleaf credentials.  

Maybe, at some point, this will yield some kind of advantage.  

For further information on using submodules see [this link](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

## Link Collection
### Recognition of Shapes
* [Ramer-Douglas-Peucker algorithm](https://de.wikipedia.org/wiki/Douglas-Peucker-Algorithmus "for contour approximation")
* [code example of opencv polygon recognition](https://stackoverflow.com/questions/15277323/opencv-shape-detection)

### Recognition of Lines
* [curved edge detection](http://www.wisdom.weizmann.ac.il/~nadler/Sublinear_Edge_Detection/Curved/Sublinear_curved_edge_detection.html)
* [EDlines paper](http://c-viz.eskisehir.edu.tr/pdfs/EDLines2011ICIP.pdf)
* [Line Segment Clustering Paper](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.38.4011&rep=rep1&type=pdf)
* [merging houghlines](https://stackoverflow.com/questions/45531074/how-to-merge-lines-after-houghlinesp)
### OCR
* [Tesseract OCR tutorial](https://medium.freecodecamp.org/getting-started-with-tesseract-part-i-2a6a6b1cf75e)
* [proprietary Web app to generate LaTeX code from vector handwriting](https://webdemo.myscript.com/views/math/index.html)
* [Example for using tesseract API in openCV](https://www.learnopencv.com/deep-learning-based-text-recognition-ocr-using-tesseract-and-opencv/)
<hr>

* [labelled handwriting dataset from NIST](https://www.nist.gov/srd/nist-special-database-19)

### TikZ resources
* [TikZ/pgf manual](http://ftp.uni-erlangen.de/ctan/graphics/pgf/base/doc/pgfmanual.pdf)  
* [Collection of TikZ related tools](http://tikz.de/tools/)
* [example of TikZ code generation in source of an inkscape plugin](https://github.com/kjellmf/svg2tikz/blob/master/svg2tikz/extensions/tikz_export.py)

### OpenCV
* [Introduction to the openCV GPU-module](https://docs.opencv.org/2.4.13.7/modules/gpu/doc/introduction.html)

### List of Software capable of exporting to TikZ
* Inkscape (open source)
* MATLAB
* matplotlib (open source)
* Blender (open source)
* Geogebra