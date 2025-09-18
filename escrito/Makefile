# Makefile para compilar la tesis en LaTeX

# Compilador de LaTeX
LATEX = pdflatex
BIBTEX = biber

# Archivo principal
MAIN = tesis

# Opciones de compilación
LATEX_OPTS = -interaction=nonstopmode -shell-escape

# Archivos auxiliares a limpiar
AUX_FILES = *.aux *.log *.out *.toc *.lof *.lot *.bbl *.blg *.fdb_latexmk *.fls *.synctex.gz

# Objetivo principal
all: $(MAIN).pdf

# Compilar el documento
$(MAIN).pdf: $(MAIN).tex
	@echo "Compilando tesis..."
	$(LATEX) $(LATEX_OPTS) $(MAIN).tex
	@echo "Tesis compilada exitosamente: $(MAIN).pdf"

# Compilar con bibliografía (si se agrega)
bib: $(MAIN).tex
	@echo "Compilando tesis con bibliografía..."
	$(BIBTEX) $(MAIN)
	$(LATEX) $(LATEX_OPTS) $(MAIN).tex
	$(LATEX) $(LATEX_OPTS) $(MAIN).tex
	@echo "Tesis con bibliografía compilada exitosamente: $(MAIN).pdf"

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

# Ver el PDF (Linux)
view-linux: $(MAIN).pdf
	xdg-open $(MAIN).pdf

# Ver el PDF (Windows)
view-windows: $(MAIN).pdf
	start $(MAIN).pdf

# Compilar y ver automáticamente
auto: $(MAIN).pdf view

# Ayuda
help:
	@echo "Opciones disponibles:"
	@echo "  all        - Compilar la tesis (por defecto)"
	@echo "  bib        - Compilar con bibliografía"
	@echo "  clean      - Limpiar archivos auxiliares"
	@echo "  clean-all  - Limpiar todo excepto el PDF"
	@echo "  view       - Ver el PDF (macOS)"
	@echo "  view-linux - Ver el PDF (Linux)"
	@echo "  view-windows - Ver el PDF (Windows)"
	@echo "  auto       - Compilar y ver automáticamente"
	@echo "  help       - Mostrar esta ayuda"

.PHONY: all bib clean clean-all view view-linux view-windows auto help
