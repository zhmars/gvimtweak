# gvimtweak

gvimtweak is a simple plugin to enable alpha, topmost, maximize, fullscreen for gvim.It is a fork of  [vim-scripts/VimTweak](https://github.com/vim-scripts/VimTweak) and [vim-scripts/gvimfullscreen_win32](https://github.com/vim-scripts/gvimfullscreen_win32).I just added a vim script for easy use


Supported platforms:
* Windows 32/64bit (Compiled by Visual Studio 2015)

## Install

### Manual Install

* Clone this repo
* Copy `lib/*` and `plugin/*` files to your 'runtimepath'
  directory (see `:help runtimepath`).

### Vim-Plug

If you use [vim-plug](https://github.com/junegunn/vim-plug), you can update automatically.

```vim
Plug 'zhmars/gvimtweak'

" if has('win32')
"   Plug 'zhmars/gvimtweak'
" endif
```

## Building(Optional)

    $ cd lib && nmake

## Preview

  ![gvimtweak preview](./preview.gif)

## Configuration Examples
```vim
" alpha value (180 ~ 255) default: 245
let g:gvimtweak#window_alpha=240

" enable alpha at startup
let g:gvimtweak#enable_alpha_at_startup=1

" enable topmost at startup
let g:gvimtweak#enable_topmost_at_startup=0

" enable maximize at startup
let g:gvimtweak#enable_maximize_at_startup=0

" enable fullscreen at startup
let g:gvimtweak#enable_fullscreen_at_startup=0

" nnoremap<silent> <A-k> :GvimTweakSetAlpha 10<CR>
" nnoremap<silent> <A-j> :GvimTweakSetAlpha -10<CR>
" nnoremap<silent> <A-t> :GvimTweakToggleTopMost<CR>
" nnoremap<silent> <A-m> :GvimTweakToggleMaximize<CR>
" nnoremap<silent> <A-f> :GvimTweakToggleFullScreen<CR>
nnoremap<silent> <F11> :GvimTweakToggleFullScreen<CR>
```

## Inspiration and special thanks
* [vim-scripts/VimTweak](https://github.com/vim-scripts/VimTweak)
* [vim-scripts/gvimfullscreen_win32](https://github.com/vim-scripts/gvimfullscreen_win32)
* [derekmcloughlin/gvimfullscreen_win32](https://github.com/derekmcloughlin/gvimfullscreen_win32)
* [asins/gvimfullscreen_win32](https://github.com/asins/gvimfullscreen_win32)
