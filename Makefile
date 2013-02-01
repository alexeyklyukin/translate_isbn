MODULES = translate_isbn
EXTENSION = translate_isbn
DATA = translate_isbn--1.0.sql
DOCS = README.translate_isbn.md
REGRESS=translate_isbn

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

