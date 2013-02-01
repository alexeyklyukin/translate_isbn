-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION translate_isbn" to load this file \quit

CREATE OR REPLACE FUNCTION translate_isbn1013(text) RETURNS TEXT
AS 'MODULE_PATHNAME', 'translate_isbn1013'
LANGUAGE C IMMUTABLE STRICT;

