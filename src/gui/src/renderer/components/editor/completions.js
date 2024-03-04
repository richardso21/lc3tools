/**
 * Author: Toby Salusky
 *
 * Description: Some relatively simple autocomplete suggestions for LC3 assembly! :D
 *
 * Autocomplete Options (store/modules/Settings.js)
 * 'none': no autocompletion
 * 'basic': builtin registers, instructions, aliases, pseudo ops
 * 'full': `basic` functionality plus best-effort label suggestion (more document processing on keystroke required)
 *
 * Issues/Areas for improvement:
 * > with autocomplete='full', string contents will be suggested as labels (really they should be ignored)
 * > do 'full' autocompletion document parsing more efficiently? (not sure how bad of an issue this currently is?)
 *
 */

const regNames = [0, 1, 2, 3, 4, 5, 6, 7].map(num => `R${num}`);
const instrs = [
  "ADD",
  "AND",
  "NOT",
  "BR", // though combinations like BRnzp exist, we only suggest BR, and simply ignore other forms from being captured as labels
  "JMP",
  "JSR",
  "JSRR",
  "LD",
  "LDI",
  "LDR",
  "LEA",
  "ST",
  "STI",
  "STR",
  "TRAP"
];
const instrAliases = ["HALT", "PUTS", "GETC", "OUT", "IN", "RET"];
const pseudoOps = ["orig", "end", "fill", "blkw", "stringz"];
const keywordSet = new Set(
  [...instrs, ...instrAliases, ...regNames, ...pseudoOps].map(s =>
    s.toLowerCase()
  )
);

function generateCompletions(mode, doc, pos, prefix) {
    if (mode === "none") {
      return [];
    }

    // typing a number
    if (/^\d/.test(prefix)) {
      return [];
    }

    // in a comment
    const currentLine = doc.getLine(pos.row);
    const currentLineUpToCursor = currentLine.substring(0, pos.column - 1);
    if (currentLineUpToCursor.includes(";")) {
      return [];
    }

    let labelsReferenced = new Set();

    if (mode == "full") {
      const documentLines = doc.getAllLines();
      for (const line of documentLines) {
        const nonCommentLine = line.includes(";")
          ? line.substring(0, line.indexOf(";"))
          : line;

        const matches = nonCommentLine.match(/\w[\d\w]*/g);
        if (matches) {
          for (const match of matches) {
            if (match === prefix) {
              continue;
            } // do not complete with what is currently being typed!

            const lowerMatch = match.toLowerCase();
            if (/^x[a-f\d]+$/.test(lowerMatch)) {
              continue;
            } // hex, not label
            if (/^\d+$/.test(lowerMatch)) {
              continue;
            } // number, not label
            if (/^brn?z?p?$/.test(lowerMatch)) {
              continue;
            } // BR, not label
            if (keywordSet.has(lowerMatch)) {
              continue;
            } // existing keyword, not label

            labelsReferenced.add(match);
          }
        }
      }
    }

    const completions = [
      ...regNames.map(name => ({
        value: name,
        score: 11,
        meta: "register"
      })),
      ...instrs.map(name => ({
        value: name,
        score: 10,
        meta: "instruction"
      })),
      ...instrAliases.map(name => ({
        value: name,
        score: 10,
        meta: "alias"
      })),
      ...pseudoOps.map(name => ({
        value: name,
        score: 10,
        meta: "pseudo-op"
      })),
      ...[...labelsReferenced].map(name => ({
        value: name,
        score: 5,
        meta: "label"
      }))
    ];

	return completions;
}

// modeGetter => returns current autocomplete setting
export const CreateLc3CompletionProvider = (modeGetter) => ({
  getCompletions: (_editor, session, pos, prefix, callback) => {
	  callback(null, generateCompletions(modeGetter(), session.doc, pos, prefix));
  }
});
