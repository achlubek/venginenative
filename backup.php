<?php
chdir(__DIR__);
passthru('gfind . | grep "\.cpp$\|\.hpp$\|\.h$\|\.c$\|\.glsl$" | tar -T - -czf code_revisions_all/backup'.count(scandir('code_revisions_all')).'.tar.gz');
