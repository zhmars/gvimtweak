if !has('win32') || !has('gui_running') || !has('libcall')
  finish
endif

if !exists('g:gvimtweak#window_alpha')
  let g:gvimtweak#window_alpha=245
endif

if !exists('g:gvimtweak#enable_alpha_at_startup')
  let g:gvimtweak#enable_alpha_at_startup=0
endif

if !exists('g:gvimtweak#enable_topmost_at_startup')
  let g:gvimtweak#enable_topmost_at_startup=0
endif

if !exists('g:gvimtweak#enable_maximize_at_startup')
  let g:gvimtweak#enable_maximize_at_startup=0
endif

if !exists('g:gvimtweak#enable_fullscreen_at_startup')
  let g:gvimtweak#enable_fullscreen_at_startup=0
endif

if !exists('g:gvimtweak#dll_path')
  let s:gvimtweak_dll_basename = has('win64') ?
        \ 'gvimtweak_win64.dll' : 'gvimtweak_win32.dll'
  let g:gvimtweak#dll_path = expand('<sfile>:p:h:h') . '\lib\' . s:gvimtweak_dll_basename
  unlet s:gvimtweak_dll_basename
endif

if !exists('g:gvimtweak#fullscreen_dll_path')
  let s:gvimtweak_fullscreen_dll_basename = has('win64') ?
        \ 'gvimfullscreen_win64.dll' : 'gvimfullscreen_win32.dll'
  let g:gvimtweak#fullscreen_dll_path = expand('<sfile>:p:h:h') . '\lib\' . s:gvimtweak_fullscreen_dll_basename
  unlet s:gvimtweak_fullscreen_dll_basename
endif

func! s:SetAlpha(alpha)
  if 180 <= a:alpha && a:alpha <= 255
    let g:gvimtweak#window_alpha = a:alpha
  else
    let g:gvimtweak#window_alpha = g:gvimtweak#window_alpha + a:alpha
    if g:gvimtweak#window_alpha < 180
      let g:gvimtweak#window_alpha = 180
    elseif g:gvimtweak#window_alpha > 255
      let g:gvimtweak#window_alpha = 255
    endif
  endif
  call libcall(g:gvimtweak#dll_path, 'SetAlpha', g:gvimtweak#window_alpha)
endf

let g:gvimtweak#topmost = g:gvimtweak#enable_topmost_at_startup ? 1 : 0
func! s:ToggleTopMost()
  let g:gvimtweak#topmost = g:gvimtweak#topmost ? 1 : 0
  call libcall(g:gvimtweak#dll_path, 'EnableTopMost', g:gvimtweak#topmost)
endf

let g:gvimtweak#maximize = g:gvimtweak#enable_maximize_at_startup ? 1 : 0
func! s:ToggleMaximize()
  let g:gvimtweak#maximize = g:gvimtweak#maximize ? 1 : 0
  call libcall(g:gvimtweak#dll_path, 'EnableMaximize', g:gvimtweak#maximize)
endf

let g:gvimtweak#fullscreen = ''
func! s:ToggleFullScreen()
  " call libcall(g:gvimtweak#dll_path, 'ToggleFullScreen', 1)
  let g:gvimtweak#fullscreen = libcall(g:gvimtweak#fullscreen_dll_path, 'ToggleFullScreen', g:gvimtweak#fullscreen)
endf

command! -nargs=1 GvimTweakSetAlpha call s:SetAlpha(<args>)
command! GvimTweakToggleTopMost call s:ToggleTopMost()
command! GvimTweakToggleMaximize call s:ToggleMaximize()
command! GvimTweakToggleFullScreen call s:ToggleFullScreen()

augroup load_gvimtweak
  autocmd!
  if g:gvimtweak#enable_alpha_at_startup
    autocmd GUIEnter * call s:SetAlpha(0)
  endif
  if g:gvimtweak#enable_topmost_at_startup
    autocmd GUIEnter * call s:ToggleTopMost()
  endif
  if g:gvimtweak#enable_maximize_at_startup
    autocmd GUIEnter * call s:ToggleMaximize()
  endif
  if g:gvimtweak#enable_fullscreen_at_startup
    autocmd GUIEnter * call s:ToggleFullScreen()
  endif
augroup END
