# Makefile para compilar la presentación en LaTeX

# Compilador de LaTeX
LATEX = pdflatex

# Archivo principal
MAIN = presentacion

# Opciones de compilación
LATEX_OPTS = -interaction=nonstopmode -shell-escape

# Archivos auxiliares a limpiar
AUX_FILES = *.aux *.log *.out *.toc *.nav *.snm *.vrb *.synctex.gz

# Objetivo principal
all: $(MAIN).pdf

# Compilar el documento (beamer requiere 2 pasadas)
$(MAIN).pdf: $(MAIN).tex
	@echo "Compilando presentación..."
	$(LATEX) $(LATEX_OPTS) $(MAIN).tex
	$(LATEX) $(LATEX_OPTS) $(MAIN).tex
	@echo "Presentación compilada exitosamente: $(MAIN).pdf"

# Limpiar archivos auxiliares
clean:
	@echo "Limpiando archivos auxiliares..."
	rm -f $(AUX_FILES)
	@echo "Limpieza completada"

# Limpiar todo excepto el PDF
clean-all: clean
	@echo "Limpiando archivos generados..."
	rm -f $(MAIN).pdf
	@echo "Limpieza completa completada"

# Ver el PDF (macOS)
view: $(MAIN).pdf
	open $(MAIN).pdf

# Compilar y ver automáticamente
auto: $(MAIN).pdf view

# Compilar con notas del presentador visibles
notes: $(MAIN)-notes.pdf

$(MAIN)-notes.pdf: $(MAIN).tex
	@echo "Compilando presentación con notas..."
	@sed 's/\\newcommand{\\notesmode}{\\notestrue}/\\newcommand{\\notesmode}{\\notestrue}\n\\notesmode/' $(MAIN).tex > $(MAIN)-notes.tex
	$(LATEX) $(LATEX_OPTS) $(MAIN)-notes.tex
	$(LATEX) $(LATEX_OPTS) $(MAIN)-notes.tex
	@rm -f $(MAIN)-notes.tex $(MAIN)-notes.aux $(MAIN)-notes.log $(MAIN)-notes.out $(MAIN)-notes.toc $(MAIN)-notes.nav $(MAIN)-notes.snm
	@echo "Presentación con notas compilada: $(MAIN)-notes.pdf"

# Ver la versión con notas
view-notes: $(MAIN)-notes.pdf
	open $(MAIN)-notes.pdf

.PHONY: all clean clean-all view auto notes view-notes

