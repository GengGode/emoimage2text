using Microsoft.AspNetCore.Mvc;
using EmoDatabaseService.Data;

namespace EmoDatabaseService.Controllers
{
    [ApiController]
    [Route("emo")]
    public class EmoImageController : ControllerBase
    {
        private readonly ILogger<EmoImageController> _logger;
        private readonly EmoDatabaseContext _context;

        public EmoImageController(EmoDatabaseContext context, ILogger<EmoImageController> logger)
        {
            _logger = logger;
            _context = context;
        }

        // return emo_image from database by ID
        [HttpGet("{ID}")]
        public JsonResult Get(Guid id)
        {
            var emoImage = _context.EmoImages.Find(id);
            if (emoImage == null)
            {
                return new JsonResult(new { status = "error", message = "image not found" });
            }
            return new JsonResult(new { status = "ok", emoImage = emoImage });
        }

        // add emo_image to database
        [HttpPost]
        public JsonResult Post(EmoImage emoImage)
        {
            // ������ͬ·���Ƿ����
            var emoImage_old = _context.EmoImages.Where(emoImage => emoImage.ImagePath == emoImage.ImagePath).FirstOrDefault();
            // ���ھ��滻
            if ( emoImage_old != null)
            {
                emoImage_old.EmoText = emoImage.EmoText;
                emoImage_old.EmoTextRaw = emoImage.EmoTextRaw;
                _context.EmoImages.Update(emoImage_old);
            }
            else
            {
                emoImage.ID = Guid.NewGuid();
                _context.EmoImages.Add(emoImage);
            }
            _context.SaveChanges();
            return new JsonResult(new { status = "ok", emoImage = emoImage });
        }

        // find emo_image from emo_image.emoTextRaw by texts
        [HttpGet("find")]
        public JsonResult Finds(string[] texts)
        {
            string[] strings = Array.Empty<string>();
            for(int i = 0; i < texts.Length; i++)
            {
                var text = texts[i];
                string[] text_strings = text.Split(' ');
                strings = strings.Concat(text_strings).ToArray();
            }
            var contains_func = new Func<EmoImage, bool>(emoImage =>
            {
                for (int i = 0; i < strings.Length; i++)
                {
                    var str = strings[i];
                    if (emoImage.EmoText.Contains(str))
                    {
                        return true;
                    }
                }
                return false;
            });

            var emoImages = _context.EmoImages.Where(contains_func).ToList();
            return new JsonResult(new { status = "ok", emoImages = emoImages });
        }

        [HttpGet("find_paths")]
        public IActionResult FindPaths(string text)
        {
            string[] strings = Array.Empty<string>();
                string[] text_strings = text.Split(' ');
                strings = strings.Concat(text_strings).ToArray();
            var contains_func = new Func<EmoImage, bool>(emoImage =>
            {
                for (int i = 0; i < strings.Length; i++)
                {
                    var str = strings[i];
                    if (emoImage.EmoText.Contains(str))
                    {
                        return true;
                    }
                }
                return false;
            });

            var emoImages = _context.EmoImages.Where(contains_func).ToList();
            var paths = emoImages.Select(emoImage => emoImage.ImagePath).ToList();
            if(paths.Count == 0)
            {
                return NotFound();
            }
            // \n split
            return Ok(string.Join("\n", paths));
        }

        // download emo_image
        [HttpGet("download/{id}")]
        public IActionResult Download(Guid id)
        {
            var emoImage = _context.EmoImages.Find(id);
            if (emoImage == null)
            {
                return NotFound();
            }
            var path = emoImage.ImagePath;
            var fileStream = new FileStream(path, FileMode.Open, FileAccess.Read);
            return File(fileStream, "application/octet-stream", Path.GetFileName(path));
        }

        // ����id����ͼƬ�ļ���������ʾͼƬ
        [HttpGet("image/{id}")]
        public IActionResult Image(Guid id)
        {
            var emoImage = _context.EmoImages.Find(id);
            if (emoImage == null)
            {
                return NotFound();
            }
            var path = emoImage.ImagePath;
            var fileStream = new FileStream(path, FileMode.Open, FileAccess.Read);
            return File(fileStream, "image/jpeg");
        }

    }
}