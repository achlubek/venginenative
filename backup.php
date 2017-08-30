<?php
chdir(__DIR__);
passthru('"C:\Program Files\Git\usr\bin\find" . | "C:\Program Files\Git\usr\bin\grep" "\.cpp$\|\.hpp$\|\.h$\|\.c$\|\.vert$\|\.frag$\|\.glsl$" | "C:\Program Files\Git\usr\bin\tar" -T - -cf code_revisions_all/backup'.count(scandir('code_revisions_all')).'.tar');
