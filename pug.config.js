module.exports = {
    filters: {
        code: (text, options) => {
            return `<pre${options.subtle ? ' class="subtle"' : ''}><code class="language-cpp">` + text.replace(/</g, "&lt;").replace(/>/g, "&gt;") + '</code></pre>';
        }
    }
}
