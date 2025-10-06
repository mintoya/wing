function inside(c1, c2, string) {
  if (!string) return string;
  if (string.value) string = string.value;
  for (let i = 0; i < string.length; i++) {
    if (string[i] === c1) {
      let counter = 1;
      for (let ii = i + 1; ii < string.length; ii++) {
        if (string[ii] === c1) {
          counter++;
        } else if (string[ii] === c2) {
          counter--;
          if (counter === 0) {
            return {
              value: string.substring(i + 1, ii), // inside only
              start: i,
              end: ii,
            };
          }
        }
      }
    }
  }
  return { value: "", start: 0, end: string.length };
}
function until(c1, string) {
  if (!string) return string;
  if (string.value) string = string.value;
  for (let i = 0; i < string.length; i++) {
    if (string[i] === c1) {
      return {
        value: string.substring(0, i),
        start: 0,
        end: i,
      };
    }
  }
  return {
    value: string,
    start: 0,
    end: string.length,
  };
}
function stringType(str) {
  str = str.trim();
  if (str[0] === "{" || str[0] === "[") {
    return "list";
  }
  let inQuotes = false;
  for (let i = 0; i < str.length; i++) {
    const ch = str[i];
    if (ch === '"') {
      if (i === 0 || str[i - 1] !== "\\") {
        inQuotes = !inQuotes;
      }
    }
    if (!inQuotes && ch === ",") {
      return "list";
    }
    if (!inQuotes && ch === ":") {
      return "object";
    }
  }
  return "object";
}
function parseArray(string) {
  string = string.trim();
  var res = [];
  while ((string = string.trim()).length) {
    switch (string[0]) {
      case "{":
        var toParse = inside("{", "}", string);
        if (string[toParse.end + 1] == ",") {
          toParse.end++;
        }
        string = string.substring(toParse.end + 1);
        res.push(parse(toParse.value.trim()));
        break;
      case "[":
        var toParse = inside("[", "]", string);
        if (string[toParse.end + 1] == ",") {
          toParse.end++;
        }
        res.push(parseArray(toParse.value.trim()));
        string = string.substring(toParse.end + 1);
        break;
      default:
        // console.log(string);
        var toParse = until(",", string);
        res.push(toParse.value.trim());
        string = string.substring(toParse.end + 1);
    }
  }
  return res;
}
function parse(string, object = {}) {
  if ((string = string.trim()) == "") return object;
  if (stringType(string) == "list") return parseArray(string);
  var name = until(":", string);
  var value = string.substring(name.end + 1).trim();
  var afterString = "";
  var parseValue = "";
  switch (value[0]) {
    case "{":
      parseValue = inside("{", "}", value);
      object[name.value.trim()] = parse(parseValue.value.trim());
      break;
    case "[":
      parseValue = inside("[", "]", value);
      object[name.value.trim()] = parse(parseValue.value.trim());
      break;
    default:
      // console.log(value[0]);
      parseValue = until(";", value);
      object[name.value.trim()] = parseValue.value;
  }
  afterString = value.substring(parseValue.end + 2);
  return parse(afterString, object);
}
function toKML(value) {
  if (Array.isArray(value)) {
    if (
      value.every((item) => typeof item === "object" && !Array.isArray(item))
    ) {
      return "[" + value.map(toKML).join(",") + "]";
    } else {
      return "[" + value.map(toKML).join(",") + "]";
    }
  } else if (typeof value === "object" && value !== null) {
    return "{" + Object.entries(value)
      .map(([key, val]) => `${key}:${toKML(val)};`)
      .join("") +
      "}";
  } else {
    if (value) {
      return String(value);
    } else return "_";
  }
}
