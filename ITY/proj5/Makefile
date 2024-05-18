SHELL = /usr/bin/env bash
NAME = proj5
LOGIN = xzvara01

projN: 
	pdflatex -shell-escape $(NAME)
	pdflatex -shell-escape $(NAME)

zip:
	zip $(LOGIN)-fit.zip *.tex ./img/* Makefile

clean:
	rm -rf *.log *.aux *.nav *.out *.snm *.toc

