namespace RemoteGod.Models
{
    public class FileItem
    {
        public string Name { get; set; }
        public string Path { get; set; }
        public bool IsDirectory { get; set; }

        public string Icon
        {
            get
            {
                if (IsDirectory)
                    return "📁";

                var ext = System.IO.Path.GetExtension(Name)?.ToLowerInvariant();
                if (string.IsNullOrEmpty(ext))
                    return "📄";

                return ext switch
                {
                    ".jpg" or ".jpeg" or ".png" or ".bmp" => "🖼️",
                    ".mp4" or ".avi" or ".mkv" => "🎬",
                    ".mp3" or ".wav" => "🎵",
                    ".txt" or ".md" => "📄",
                    ".pdf" => "📕",
                    ".doc" or ".docx" => "📄",
                    ".xls" or ".xlsx" => "📊",
                    ".exe" => "⚙️",
                    ".zip" or ".rar" => "🗜️",
                    _ => "📄"
                };
            }
        }

        public string DisplayName => Name;
    }
}
