import Reveal from 'reveal.js';
import RevealHighlight from 'reveal.js/plugin/highlight/highlight';
import cpp from 'highlight.js/lib/languages/cpp';

let deck = new Reveal({
    plugins: [RevealHighlight],
    highlight: {
        beforeHighlight: hljs => hljs.registerLanguage("c++", cpp)
    },
    slideNumber: 'c/t',
    margin: 0,
    width: 1920,
    height: 1080,
    center: false,
    hash: true,
});
deck.initialize();
