<?php
use PHPMailer\PHPMailer\PHPMailer;
require '/home/ariela/repos/viewmind_projects/Scripts/php/vendor/autoload.php';
$mail = new PHPMailer;
$mail->isSMTP();
$mail->setFrom('check.viewmind@gmail.com', 'ViewMind Administration');
$mail->Username = 'AKIARDLQA5AHRSTLPCYS';
$mail->Password = 'BHGWozyNwZoHjvUAhL8d7H9FC/H4RBNfh564MnKZRKj/';
$mail->Host = 'email-smtp.us-east-1.amazonaws.com';
$mail->Subject = 'ViewMind EyeServer Stopped Working';
$mail->addAddress('aarelovich@gmail.com', 'Ariel Arelovich');
$mail->Body = 'The EyeServer Exited with code: 15 and an exit status CrashExit';
$mail->SMTPAuth = true;
$mail->SMTPSecure = 'tls';
$mail->Port = 587;
$mail->isHTML(true);
if(!$mail->send()) {
    echo 'Email error is: ' , $mail->ErrorInfo , PHP_EOL;
}
?>
