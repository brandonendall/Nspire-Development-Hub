"""Delimited text files with Excel-style quoting."""

QUOTE_MINIMAL = 0
QUOTE_ALL = 1
QUOTE_NONE = 3


class Error(Exception):
    pass


def _validate(delimiter, quotechar, quoting):
    for value in (delimiter, quotechar):
        if not isinstance(value, str) or len(value) != 1 or value in "\r\n":
            raise TypeError("delimiter and quotechar must be single characters")
    if delimiter == quotechar:
        raise ValueError("delimiter and quotechar must differ")
    if quoting not in (QUOTE_MINIMAL, QUOTE_ALL, QUOTE_NONE):
        raise ValueError("unsupported quoting mode")


class reader:
    def __init__(self, lines, *, delimiter=",", quotechar='"', quoting=QUOTE_MINIMAL,
                 skipinitialspace=False, strict=False):
        _validate(delimiter, quotechar, quoting)
        self._lines = iter(lines)
        self._delimiter = delimiter
        self._quote = quotechar if quoting != QUOTE_NONE else None
        self._skip_space = skipinitialspace
        self._strict = strict
        self.line_num = 0

    def __iter__(self):
        return self

    def __next__(self):
        fields = []
        field = []
        in_quotes = False
        started = False
        after_quote = False
        for line in self._lines:
            if not isinstance(line, str):
                raise Error("iterator must return text strings")
            self.line_num += 1
            index = 0
            while index < len(line):
                ch = line[index]
                index += 1
                if in_quotes:
                    if ch == self._quote:
                        if index < len(line) and line[index] == self._quote:
                            index += 1
                        else:
                            in_quotes = False
                            after_quote = True
                            continue
                    field.append(ch)
                elif ch == self._delimiter:
                    fields.append("".join(field))
                    field = []
                    started = after_quote = False
                elif ch in "\r\n":
                    if line[index:].strip("\r\n"):
                        raise Error("unquoted newline inside input line")
                    break
                elif not started and self._skip_space and ch == " ":
                    continue
                elif not started and ch == self._quote:
                    started = in_quotes = True
                else:
                    if after_quote and self._strict:
                        raise Error("expected delimiter after closing quote")
                    field.append(ch)
                    started = True
            if not in_quotes:
                if started or fields or (self._skip_space and line.startswith(" ")):
                    fields.append("".join(field))
                return fields
        if in_quotes:
            if self._strict:
                raise Error("unterminated quoted field")
            fields.append("".join(field))
            return fields
        raise StopIteration


class writer:
    def __init__(self, file, *, delimiter=",", quotechar='"', quoting=QUOTE_MINIMAL,
                 lineterminator="\r\n"):
        _validate(delimiter, quotechar, quoting)
        if not isinstance(lineterminator, str):
            raise TypeError("lineterminator must be a string")
        self._write = file.write
        self._delimiter = delimiter
        self._quote = quotechar
        self._quoting = quoting
        self._ending = lineterminator

    def writerow(self, row):
        fields = ["" if value is None else str(value) for value in row]
        special = self._delimiter + self._quote + "\r\n" + self._ending
        for index, field in enumerate(fields):
            needs_quotes = (len(fields) == 1 and not field) or any(c in special for c in field)
            if needs_quotes and self._quoting == QUOTE_NONE:
                raise Error("field requires quoting")
            if needs_quotes or self._quoting == QUOTE_ALL:
                quote = self._quote
                fields[index] = quote + field.replace(quote, quote + quote) + quote
        return self._write(self._delimiter.join(fields) + self._ending)

    def writerows(self, rows):
        for row in rows:
            self.writerow(row)


class DictReader:
    def __init__(self, lines, fieldnames=None, restkey=None, restval=None, **options):
        self.reader = reader(lines, **options)
        self._fieldnames = None if fieldnames is None else list(fieldnames)
        self.restkey = restkey
        self.restval = restval

    @property
    def fieldnames(self):
        if self._fieldnames is None:
            self._fieldnames = next(self.reader, None)
        return self._fieldnames

    @property
    def line_num(self):
        return self.reader.line_num

    def __iter__(self):
        return self

    def __next__(self):
        names = self.fieldnames
        row = next(self.reader)
        while not row:
            row = next(self.reader)
        result = dict(zip(names, row))
        if len(row) > len(names):
            result[self.restkey] = row[len(names):]
        elif len(row) < len(names):
            for name in names[len(row):]:
                result[name] = self.restval
        return result


class DictWriter:
    def __init__(self, file, fieldnames, restval="", extrasaction="raise", **options):
        self.fieldnames = list(fieldnames)
        self.restval = restval
        self._extrasaction = extrasaction.lower()
        if self._extrasaction not in ("raise", "ignore"):
            raise ValueError("extrasaction must be raise or ignore")
        self.writer = writer(file, **options)

    def writeheader(self):
        return self.writer.writerow(self.fieldnames)

    def writerow(self, row):
        if self._extrasaction == "raise" and any(name not in self.fieldnames for name in row):
            raise ValueError("row contains fields outside fieldnames")
        return self.writer.writerow(row.get(name, self.restval) for name in self.fieldnames)

    def writerows(self, rows):
        for row in rows:
            self.writerow(row)
