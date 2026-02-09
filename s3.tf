resource "aws_s3_bucket" "raw" {
  bucket = "${var.project}-${var.env}-raw-data"
}

resource "aws_s3_bucket_versioning" "raw_versioning" {
  bucket = aws_s3_bucket.raw.id
  versioning_configuration { status = "Enabled" }
}
