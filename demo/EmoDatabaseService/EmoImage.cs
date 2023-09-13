namespace EmoDatabaseService
{
    public class EmoImage
    {
        public Guid ID { get; set; } = Guid.Empty;

        public string ImageName { get; set; } = "";

        public string ImagePath { get; set; } = "";

        public string ImageBase64 { get; set; } = "";

        public int ImageWidth { get; set; }

        public int ImageHeight { get; set; }

        public string EmoText { get; set; } = "";

        public string EmoTextRaw { get; set; } = "";

    }
}