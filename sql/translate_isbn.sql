CREATE EXTENSION translate_isbn;

-- proper ISBN13 code
SELECT translate_isbn1013('9783161484100');
-- make sure we can skip insignificant chars
SELECT translate_isbn1013('9971-5-0210-0');
-- make sure 'x' will be uppercased in a result
SELECT translate_isbn1013('0-9752298-0-x');
-- test insignificat chars for ISBN13 as well
SELECT translate_isbn1013('978-0-306-40615-7');
-- ISBN13 not stared with 978, make sure it's not converted to ISBN10
SELECT translate_isbn1013('0000000000000');
-- Invalid number of digits (11), the code should be displayed as is
SELECT translate_isbn1013('00000000000');
-- Make sure we can skip insignificant characters
SELECT translate_isbn1013('foo123-45-baz678-90bar');
-- Make sure only trailing X is allowed for ISBN 10
SELECT translate_isbn1013('X987654321');
-- Make sure only code that is 10 or 13 characters is considered valud
SELECT translate_isbn1013('921043215XX');
-- How do we deal with codes that are totally bogus?
SELECT translate_isbn1013('bogusvalue');
-- Make sure we can process strings that consist of multiple values.
select translate_isbn1013('9785431249023 43293489X 12-dd-34-56-zz-21-9X foobar 978212043210X');



