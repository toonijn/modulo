function parseClasses(classes) {
  const c = Object.entries(classes)
    .filter(([k, v]) => v)
    .map(([k, v]) => k)
    .join(" ");
  return c ? `class="${c}"` : "";
}

module.exports = {
  filters: {
    code: (text, options) => {
      return (
        `<pre data-compile="${options.compile}" ${parseClasses({
          subtle: options.subtle,
          compile: !!options.compile,
        })}>` +
        `<code class="language-cpp" contenteditable="${!!options.compile}">` +
        text.replace(/</g, "&lt;").replace(/>/g, "&gt;") +
        "</code></pre>"
      );
    },
  },
};
