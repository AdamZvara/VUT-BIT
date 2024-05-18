using System;
using System.Threading.Tasks;

namespace RideWithMe.App.ViewModels
{
    public interface IDetailViewModel<out TDetail>
    {
        TDetail? Model { get; }
        Task LoadAsync(Guid id);
        Task DeleteAsync();
        Task SaveAsync();
    }
}