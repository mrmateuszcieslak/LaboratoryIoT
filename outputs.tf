output "iot_endpoint" {
  description = "IoT Core ATS endpoint"
  value       = data.aws_iot_endpoint.iot.endpoint_address
}

output "certificate_pem" {
  description = "Certyfikat urządzenia (PEM)"
  value       = aws_iot_certificate.cert.certificate_pem
  sensitive   = true
}

output "private_key" {
  description = "Klucz prywatny urządzenia"
  value       = aws_iot_certificate.cert.private_key
  sensitive   = true
}
