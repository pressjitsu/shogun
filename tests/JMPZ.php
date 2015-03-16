<?php
	if (false) {
		$a = 1;
		goto out;
	} else {
		$a = 2;
	}

	out:
		return 8;
