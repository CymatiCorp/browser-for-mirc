; mchef attach $window(-3).hwnd
; Attach mChef to the mIRC background.

alias test {
  window -de @test
  mchef select $window(@test).hwnd
  ; mchef navigate file:/// $+ $mchef.urlencode($scriptdir $+ test.html)
  mchef navigate https://www.google.com/
}

alias mChef.C_REPLY {
  echo -s $1-
  return $iif($gettok($1,1,32) == C_OK,$true,$false)
}

alias -l mChef.sep { return $chr(124) }
alias -l mChef.dll { return $+($scriptdir $+ mChef.dll) }

alias mChef.debug {
  if ($1 == on) { set %mChef.debug $true }
  elseif ($1 == off) { unset %mChef.debug }
  elseif (%mChef.debug) {
    if (!$window(@mChef.debug)) {
      window -e @mChef.debug
    }
    echo -t @mChef.debug $1-
  }
}

alias mChef.dllcall {
  var %mChef.ret $dll($mChef.dll,$1,$2-)
  mChef.debug $+($,dll $chr(2),$chr(3),04=>,$chr(3),99,$chr(2)) $1 $+($chr(2),=>,$chr(2)) $2-
  mChef.debug $+($,dll $chr(2),$chr(3),04<=,$chr(3),99,$chr(2)) %mChef.ret
  return %mChef.ret
}

alias mchefmark {
  noop $mChef.dllcall(Mark,$+($1,$mchef.sep,$2-))
}

alias mChefx {
  if ($prop == mark) {
    return $gettok($mChef.dllcall(Mark,$1),4-,32)
  }
  elseif ($prop == attached) {
    return $mChef.C_REPLY($mChef(IsAttached,$1))
  }
}

alias mChef {
  if ($1 == init) {
    $iif($isid,return,noop) $mChef.dllcall(mChef_Init,$iif($2,$ifmatch))
  }
  elseif ($1 == attach) {
    set %mChef.awnd $2
    $iif($isid,return,noop) $mChef.dllcall(Attach,$+($2,$mChef.sep,h,$2,$mChef.sep,mChef.handler,$mChef.sep,about:blank))
  }
  elseif ($1 == navigate) {
    if ($mChefx(%mChef.awnd).attached) {
      ;If the window is already attached
      $iif($isid,return,noop) $mChef.dllcall(Navigate,$+(%mChef.awnd,$mChef.sep,$2-))
      ;Navigate to the URL
    }
    else {
      ;Attach and navigate to the URL
      $iif($isid,return,noop) $mChef.dllcall(Attach,$+(%mChef.awnd,$mChef.sep,h,%mChef.awnd,$mChef.sep,mChef.handler,$mChef.sep,$2-))
    }
  }
  elseif ($1 == mark) {
    $iif($isid,return,noop) $mChef.dllcall(Mark,%mChef.awnd $+ $iif(!$ifmatch,$mChef.sep $+ $2-))
  }
  elseif ($1 == print) {
    $iif($isid,return,noop) $mChef.dllcall(PrintWindow,%mChef.awnd)
  }
  elseif ($1 == execscript) {
    $iif($isid,return,noop) $mChef.dllcall(ExecScript,$+(%mChef.awnd,$mChef.sep,$2-))
  }
  elseif ($1 == select) {
    set %mChef.awnd $2
  }
  elseif ($1 == isattached) {
    $iif($isid,return,noop) $mChef.dllcall(IsAttached,$2-)
  }
  elseif ($1 == addhtml) {
    ;$iif($isid,return,noop) $mChef.dllcall(ExecScript,$+(%mChef.awnd,$mChef.sep,document.body.innerHTML+=",$mChef.escJS($2-),";))
    ; $iif($isid,return,noop) $mChef.dllcall(ExecScript,$+(%mChef.awnd,$mChef.sep,appendHTML(",$mChef.escJS($2-),");))
  }
  else {
    echo -cat Info * mChef: Unknown Command: $1-
  }
}

alias mChef.handler {
  echo -s $1-
  echo -st mChef.Handler $1-
  if ($2 == TitleChanged) {
    ;return mChef - $3-
  }
  elseif ($2 == NavigateBegin) {
    if ($left($5,16) == http://mirc.exe/) {
      ; $eval($urldecode($right($5-,-16)),2)
      return c_cancel
    }
    else {
      return
    }
  }
  elseif ($2 == JSBind) {
    ;Allow mChef javascript object only on file: or about: protocols due to security issues.
    if ($left($3,6) == about:) || ($left($3,7) == file://) {
      return C_OK
    }
    else {
      return C_OK
      return C_CANCEL
    }
  }
}

alias mChef.escJS { return $replace($1-,\,\\\\,|,\|,",\",',\',$chr(9),\t,$cr,\r,$lf,\n,$chr(12),\f,$chr(8),\b) }

;URL encoding identifiers from http://www.mircscripts.org/showdoc.php?type=code&id=3256 - Modified props.
alias urlencode return $regsubex($1-,/\G(.)/g,$iif((!$prop && \1 !isalnum) || $prop,$chr(37) $+ $base($asc(\1),10,16),\1))
alias urldecode return $replace($regsubex($1-,/%(\w\w)/g,$chr($iif($base(\t,16,10) != 32,$v1,1))),$chr(1),$chr(32))
