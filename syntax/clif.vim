" Vim syntax file
" Language:	CLIF
" Maintainer:	Julien Blitte <julien.blitte@gmail.com>
" Version:	0.1
" Last Change:	2017 July 2017

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Load c syntax
runtime! syntax/c.vim
syn keyword cType clif_grammar clif_keywork

"syn keyword clifSyntax %} %{
syn match clifSeparator /%{\|%}/ containedin=cParen,cBracket,cBadBlock

syn region clifRegion matchgroup=Delimiter start="%{" end="%}" contains=@clifCode contained
syn match clifCallback /->/ containedin=clifRegion

syn cluster clifCode contains=clifVariable
syn match clifVariable /\$\w\+/ containedin=clifRegion

hi def link clifSeparator	Macro
hi def link clifCallback	Operator
hi def link clifVariable	Variable

highlight clifCallback ctermfg=darkyellow
highlight clifVariable ctermfg=lightblue

let b:current_syntax = "clif"
" vim: ts=4

" does not work :-(
au BufNewFile,BufRead *.clif set filetype=clif

