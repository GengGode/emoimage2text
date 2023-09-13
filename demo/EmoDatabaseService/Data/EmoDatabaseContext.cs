using EmoDatabaseService.Controllers;
using Microsoft.EntityFrameworkCore;

namespace EmoDatabaseService.Data
{
    public class EmoDatabaseContext : DbContext
    {
        public EmoDatabaseContext(DbContextOptions<EmoDatabaseContext> options) : base(options)
        {
        }

        public DbSet<EmoImage> EmoImages { get; set; }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            modelBuilder.Entity<EmoImage>().ToTable("EmoImage");
        }
    }
}