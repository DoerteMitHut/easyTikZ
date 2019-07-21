function [img_o] = binarize(img)
    %converting to gray
    g = img;
    if (size(img,3)~=1)
        g = rgb2gray(img);
    end
    %histogram for scale estimation
    [count,x] = imhist(g);
    q = sum(count(1:100))/sum(count(100:end));
    %q = 0.0208 /q;
    q = 0.019 /q;
    grad = 255-imgaussfilt(g,200*q);
    %imshow(grad);
    %pause(1);
    g = (g+grad)/2;
    %imshow(g)
    %pause(1)
    g = imgaussfilt(g,3*q);
    %imshow(g)
    %pause(1)
    [count,x] = imhist(g,256);
    t = otsuthresh(count)*255;
    img_o = double(g>=t);
end