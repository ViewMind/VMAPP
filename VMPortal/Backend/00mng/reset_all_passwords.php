<?php

  $con = mysqli_connect("localhost","root","givnar","vm_secure");
  $password = password_hash("1234", PASSWORD_BCRYPT, ["cost" => 10]);

  $sql = "UPDATE portal_users SET PASSWD = '$password'";
  mysqli_query($con,$sql);
  

?>
