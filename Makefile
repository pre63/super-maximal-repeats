.PHONY: install tests fix

install:
	@python3 -m venv .venv
	@. .venv/bin/activate && pip install -r requirements.txt
	@. .venv/bin/activate && pip install --upgrade pip

install_package:
	@. .venv/bin/activate && pip install -e .

download:
	@. .venv/bin/activate && python scripts/dataset.py

tests: fix download install_package
	@. .venv/bin/activate && pytest -s tests/

fix:
	@. .venv/bin/activate && black . && isort .
	@clang-format -i supermaxrep/smr.cpp