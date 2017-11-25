CO=report

all: $(CO).pdf

pdf: $(CO).pdf

$(CO).ps: $(CO).dvi
	dvips $(CO)

draft: $(CO)*.tex
	pdflatex $(CO)

$(CO).pdf: clean
	pdflatex -draftmode $(CO)
	bibtex $(CO)
	pdflatex -draftmode $(CO)
	pdflatex $(CO)

$(CO).dvi: $(CO).tex $(CO).bib
	latex $(CO)
	bibtex $(CO)
	latex $(CO)
	latex $(CO)

clean:
	rm -f *.dvi *.log $(CO).blg $(CO).bbl $(CO).toc *.aux $(CO).out $(CO).lof $(CO).ptc
	rm -f $(CO).pdf

pack:
	tar czvf $(CO).tar.gz *.tex *.bib *.bst $(CO).pdf Makefile

# count of standard pages
norm:
	echo "scale=2; `detex -n content.tex | wc -c`/1800;" | bc

