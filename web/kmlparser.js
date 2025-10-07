globalThis.fromKML = (input) => {
  let i = 0;
  const s = input;
  const len = s.length;
  function skipWS() {
    while (i < len && /\s/.test(s[i])) i++;
  }
  function peek() {
    return s[i];
  }
  function parseKey() {
    skipWS();
    const start = i;
    while (i < len && /[A-Za-z0-9_]/.test(s[i])) i++;
    return s.slice(start, i);
  }
  function parseValue(stopChars = [";", "}"]) {
    skipWS();
    if (i >= len) return "";
    const ch = s[i];
    if (ch === "{") return parseObject();
    if (ch === "[") return parseArray();
    // bare token (may include parentheses like M( ls ) )
    return parseTokenUntil(stopChars);
  }
  // parse a token until one of stopChars is found at top-level (not inside parentheses)
  function parseTokenUntil(stopChars) {
    skipWS();
    let start = i;
    let levelParen = 0;
    while (i < len) {
      const c = s[i];
      if (c === "(") {
        levelParen++;
        i++;
        continue;
      }
      if (c === ")") {
        if (levelParen > 0) levelParen--;
        i++;
        continue;
      }
      if (levelParen === 0 && stopChars.includes(c)) break;
      i++;
    }
    return s.slice(start, i).trim();
  }
  function parseArray() {
    // assumes s[i] === '['
    i++; // consume '['
    const arr = [];
    skipWS();
    while (i < len) {
      skipWS();
      if (s[i] === "]") {
        i++;
        break;
      }
      if (s[i] === "{") arr.push(parseObject());
      else if (s[i] === "[") arr.push(parseArray());
      else {
        // parse an item until top-level comma or closing bracket
        const item = parseTokenUntil([",", "]"]);
        if (item !== "") arr.push(item);
      }
      skipWS();
      if (s[i] === ",") {
        i++;
        continue;
      }
      // if ']' the loop will exit on next iteration
    }
    return arr;
  }
  function parseObject() {
    // assumes s[i] === '{'
    i++; // consume '{'
    const obj = {};
    skipWS();
    while (i < len) {
      skipWS();
      if (s[i] === "}") {
        i++;
        break;
      }
      const key = parseKey();
      skipWS();
      if (s[i] === ":") i++; // consume ':'
      const val = parseValue([";", "}"]);
      obj[key] = val;
      skipWS();
      // object field separators in your input appear as ';' (but we accept ',' too)
      if (s[i] === ";" || s[i] === ",") {
        i++;
        continue;
      }
      // if next is '}' the loop will end
    }
    return obj;
  }
  // Top-level parse (sequence of key:value pairs separated by ';')
  const result = {};
  while (i < len) {
    skipWS();
    if (i >= len) break;
    const key = parseKey();
    if (!key) {
      i++;
      continue;
    }
    skipWS();
    if (s[i] === ":") i++;
    else { /* malformed but keep going */ }
    const value = parseValue([";"]);
    result[key] = value;
    skipWS();
    // consume ';' if present
    if (s[i] === ";") i++;
  }
  return result;
};
globalThis.toKML = (value) => {
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
};
