\ProvidesClass{prodrive/prodrive}

\LoadClass{report}

\usepackage{prodrive/package/pdlayout}    % place at top otherwise package pdheader won't work correctly.
\usepackage{prodrive/package/pdlanguage}  % placed before package 'pdattribute' otherwise package 'datetime' won't work correctly.
\usepackage{prodrive/package/pdtitlepage} % placed before package 'pdcolor' since it breaks usage of tikz
\usepackage{prodrive/package/pdheader}    % placed before package 'pdcolor' since it breaks usage of tikz

\usepackage{prodrive/package/pdattribute}
\usepackage{prodrive/package/pdappendix}
\usepackage{prodrive/package/pdbibliography}
\usepackage{prodrive/package/pdcolor}
\usepackage{prodrive/package/pddistributionlist}
\usepackage{prodrive/package/pddocumenthistory}
\usepackage{prodrive/package/pdfigure}
\usepackage{prodrive/package/pdfunction}
\usepackage{prodrive/package/pdfont}
\usepackage{prodrive/package/pdheading}
\usepackage{prodrive/package/pdissue}
\usepackage{prodrive/package/pdlandscape}
\usepackage{prodrive/package/pdlinenumber}
\usepackage{prodrive/package/pdlist}
\usepackage{prodrive/package/pdlisting}
\usepackage{prodrive/package/pdlongtable}
\usepackage{prodrive/package/pdmarking}
\usepackage{prodrive/package/pdreference}
\usepackage{prodrive/package/pdreq}
\usepackage{prodrive/package/pdtable}
\usepackage{prodrive/package/pdtabular}
\usepackage{prodrive/package/pdtabularx}
\usepackage{prodrive/package/pdtest}
\usepackage{prodrive/package/pdtext}
\usepackage{prodrive/package/pdtextlabel}
\usepackage{prodrive/package/pdtoc}
\usepackage{prodrive/package/pdtool}
\usepackage{prodrive/package/pdhwqrd}