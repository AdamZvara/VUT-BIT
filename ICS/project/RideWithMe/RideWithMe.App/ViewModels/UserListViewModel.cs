using System;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows.Input;
using Microsoft.Extensions.Options;
using RideWithMe.App.Commands;
using RideWithMe.App.Extensions;
using RideWithMe.App.Messages;
using RideWithMe.App.Messages.ViewMessages;
using RideWithMe.App.Services;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Wrappers;
using RideWithMe.BL.Facades;
using RideWithMe.BL.Models;

namespace RideWithMe.App.ViewModels;

public class UserListViewModel : ViewModelBase, IUserListViewModel
{
    private readonly UserFacade _userFacade;
    private readonly IMediator _mediator;
    private readonly ILoggedInUser _loggedInUserService;

    public UserListViewModel(
        UserFacade userFacade, 
        IMediator mediator,
        ILoggedInUser loggedInUserService,
        IUserMenuViewModel userMenuViewModel)
    {
        _userFacade = userFacade;
        _mediator = mediator;
        _loggedInUserService = loggedInUserService;
        UserMenuViewModel = userMenuViewModel;

        // Commands for sending messages about UI actions
        UserNewCommand = new RelayCommand(UserNew);
        UserSelectedCommand = new AsyncRelayCommand<UserModel>(SelectUser);
        LostFocusCommand = new AsyncRelayCommand(OnLostFocus);


        // When new user is created, update user collection
        _mediator.Register<UpdateMessage<UserWrapper>>(UpdateUserMessage);
        _mediator.Register<RefreshMessage<UserWrapper>>(OnAppRefresh);
    }

    public ICommand UserNewCommand { get; }
    public ICommand UserSelectedCommand { get; }
    public ICommand LostFocusCommand { get; }

    public IUserMenuViewModel UserMenuViewModel { get; }

    public ObservableCollection<UserModel> Users { get; } = new();

    private void UserNew() => _mediator.Send(new OpenUserDetailMessage<UserWrapper>());
    private async void UpdateUserMessage(UpdateMessage<UserWrapper> _) => await LoadAsync();

    private async Task SelectUser(UserModel? user)
    {
        if (user == null) return;

        // Store logged in user Guid
        _loggedInUserService.SetLoggedUserGuid(user.Id);
        await UserMenuViewModel.LoadAsync(user.Id);
        _mediator.Send(new SelectedMessage<UserWrapper> {Id = user?.Id});
        _mediator.Send(new UpdateMessage<CarWrapper>());
    }
    public async Task LoadAsync()
    {
        Users.Clear();
        var users = await _userFacade.GetAllAsync();
        Users.AddRange(users);
    }

    private async void OnAppRefresh(RefreshMessage<UserWrapper> _)
    {
        await LoadAsync();
    }

    private async Task OnLostFocus()
    {
        await LoadAsync();
    }

}