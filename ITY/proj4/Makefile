NAME=proj4
LOGIN=xzvara01

compile: 
	latex $(NAME).tex
	bibtex $(NAME)
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps

zip:
	zip $(LOGIN)-fit.zip $(NAME).tex Makefile *.bib *.bst

clean:
	rm -f $(NAME).{aux,out,dvi,ps,log}
