using RideWithMe.App.Enums;

namespace RideWithMe.App.Model;

public class FilterModel
{
    public FilterOption Name { get; set; }

    public string NameStr
    {
        get => Name.ToString();
    }
    public bool Checked { get; set; }
    public string Value { get; set; }
}