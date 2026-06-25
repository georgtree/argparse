package require ruff
package require fileutil

set docDir [file dirname [file normalize [info script]]]
set sourceDir [file join $docDir ..]
source [file join $docDir argparse.ruff]

set packageVersion [package versions argparse]
puts $packageVersion
set title "argparse package"

set commonSphinx [list -title $title -sortnamespaces false -pagesplit namespace -recurse false\
                    -includesource false -pagesplit none -autopunctuate true -compact false -includeprivate false\
                    -product argparse -diagrammer "ditaa --border-width 1" -version $packageVersion\
                    -copyright "George Yashin" {*}$::argv]
set commonNroff [list -title $title -sortnamespaces false -pagesplit namespace -recurse false\
                         -pagesplit none -autopunctuate true -compact false -includeprivate false\
                         -product argparse -diagrammer "ditaa --border-width 1" -version $packageVersion\
                         -copyright "George Yashin" {*}$::argv]

set namespaces [list ::argparse]

ruff::document $namespaces -format sphinx -outfile argparse.rst -outdir [file join $docDir sphinx]\
        {*}$commonSphinx
ruff::document $namespaces -format nroff -outdir $docDir -outfile argparse.n {*}$commonNroff

::fileutil::appendToFile [file join $docDir sphinx conf.py] {html_theme = "classic"
extensions = [
    "sphinx.ext.githubpages",
]
from pygments.lexers.tcl import TclLexer
from pygments.token import Operator

class MyTclLexer(TclLexer):
    def get_tokens_unprocessed(self, text):
        for i, t, v in super().get_tokens_unprocessed(text):
            if v == "=":
                yield i, Operator, v   # or Name.Builtin
            elif v == "$":
                yield i, Operator, v   # or Name.Builtin
            elif v == "\\":
                yield i, Operator, v   # or Name.Builtin
            elif v == "%":
                yield i, Operator, v   # or Name.Builtin
            elif v == "'":
                yield i, Operator, v   # or Name.Builtin
            else:
                yield i, t, v

def setup(app): 
    from sphinx.highlighting import lexers
    lexers["tcl"] = MyTclLexer()
}

catch {exec sphinx-build -b html [file join $docDir sphinx] [file join $docDir]} errorStr
puts $errorStr

