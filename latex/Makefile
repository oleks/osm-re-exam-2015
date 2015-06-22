.phony: all clean

all: main.pdf

main.pdf: main.tex heap-insert.tex
	pdflatex main.tex
	pdflatex main.tex

clean:
	rm -f main.pdf
	rm -f *.log
	rm -f *.aux
