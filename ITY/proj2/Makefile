NAME=proj2
LOGIN=xzvara01

compile: 
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps

zip:
	zip $(LOGIN)-fit.zip $(NAME).tex Makefile

clean:
	rm -f $(NAME).{aux,out,dvi,ps,log}
